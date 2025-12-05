from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess, uuid, os, json
import re
from copy import deepcopy

app = Flask(__name__)
CORS(app)

COMPILER = "/root/rust-compiler/a.out"      # tu compilador C++ → genera assembly .s
WORKDIR = "tmp"
os.makedirs(WORKDIR, exist_ok=True)


# ============================================================
#  SIMULADOR INTEGRADO (copiado del simulator.py)
# ============================================================

def initial_state():
    return {
        "registers": {
            "RAX": 0, "RBX": 0, "RCX": 0, "RDX": 0,
            "RSI": 0, "RDI": 0,
            "RSP": 0x7fffffffdff8,
            "RBP": 0x7fffffffe000,
        },
        "memory": {}
    }


def mem_read(state, addr):
    return state["memory"].get(addr, 0)


def mem_write(state, addr, val):
    state["memory"][addr] = val & 0xffffffffffffffff


REGS = {
    "%rax": "RAX", "%rbx": "RBX", "%rcx": "RCX", "%rdx": "RDX",
    "%rdi": "RDI", "%rsi": "RSI", "%rbp": "RBP", "%rsp": "RSP"
}


def parse_reg(t):
    return REGS.get(t.lower())


def parse_imm(t):
    return int(t.replace("$", ""))


def parse_mem(token, state):
    m = re.match(r'(-?\d*)\(%([a-z0-9]+)\)', token)
    offset_str, reg = m.groups()
    offset = int(offset_str) if offset_str else 0
    base = state["registers"][reg.upper()]
    return (base + offset) & 0xffffffffffffffff


# -------- INSTRUCCIONES --------
def exec_pushq(state, op):
    r = parse_reg(op)
    val = state["registers"][r]
    state["registers"]["RSP"] -= 8
    mem_write(state, state["registers"]["RSP"], val)


def exec_popq(state, op):
    r = parse_reg(op)
    val = mem_read(state, state["registers"]["RSP"])
    state["registers"][r] = val
    state["registers"]["RSP"] += 8


def exec_movq(state, src, dst):
    if src.startswith("$"):
        val = parse_imm(src)
    elif "(" in src:
        val = mem_read(state, parse_mem(src, state))
    else:
        val = state["registers"][parse_reg(src)]

    if "(" in dst:
        mem_write(state, parse_mem(dst, state), val)
    else:
        state["registers"][parse_reg(dst)] = val


def exec_addq(state, op1, op2):
    v1 = parse_imm(op1) if op1.startswith("$") else state["registers"][parse_reg(op1)]
    r2 = parse_reg(op2)
    state["registers"][r2] = (state["registers"][r2] + v1) & 0xffffffffffffffff


def exec_subq(state, op1, op2):
    v1 = parse_imm(op1) if op1.startswith("$") else state["registers"][parse_reg(op1)]
    r2 = parse_reg(op2)
    state["registers"][r2] = (state["registers"][r2] - v1) & 0xffffffffffffffff


def exec_leave(state):
    state["registers"]["RSP"] = state["registers"]["RBP"]
    val = mem_read(state, state["registers"]["RSP"])
    state["registers"]["RBP"] = val
    state["registers"]["RSP"] += 8


def exec_ret(state):
    state["registers"]["RSP"] += 8


def parse_line(line):
    line = line.split("#")[0].strip()
    if not line:
        return None

    t = line.replace(",", " ").split()
    op = t[0]

    if op in ("pushq", "popq"):
        return (op, [t[1]])
    if op in ("movq", "addq", "subq"):
        return (op, [t[1], t[2]])
    if op in ("leave", "ret"):
        return (op, [])
    return None


def format_state(step, inst, state):
    regs = {r: f"0x{state['registers'][r]:016x}" for r in state["registers"]}

    rsp = state["registers"]["RSP"]
    stack = []
    for i in range(10):
        addr = rsp + i * 8
        stack.append({
            "address": f"0x{addr:016x}",
            "value": f"0x{mem_read(state, addr):016x}"
        })

    return {
        "instruction": inst,
        "registers": regs,
        "stack": stack
    }


def run_simulator(asm_lines):
    trace = []
    state = initial_state()

    for step, line in enumerate(asm_lines):
        parsed = parse_line(line)
        if not parsed:
            continue

        inst, ops = parsed
        trace.append(format_state(step, line.strip(), deepcopy(state)))

        if inst == "pushq": exec_pushq(state, ops[0])
        elif inst == "popq": exec_popq(state, ops[0])
        elif inst == "movq": exec_movq(state, ops[0], ops[1])
        elif inst == "addq": exec_addq(state, ops[0], ops[1])
        elif inst == "subq": exec_subq(state, ops[0], ops[1])
        elif inst == "leave": exec_leave(state)
        elif inst == "ret": exec_ret(state)

    trace.append(format_state(len(trace), "(end)", deepcopy(state)))
    return trace


# ============================================================
#  RUTA /compile → llamada por Streamlit
# ============================================================

@app.post("/compile")
def compile_and_simulate():
    try:
        code = request.json.get("code", "")

        uid = str(uuid.uuid4())
        infile = f"{WORKDIR}/{uid}.rs"
        outfile = infile.replace(".rs", ".s")

        # escribir archivo de entrada
        with open(infile, "w") as f:
            f.write(code)

        # ejecutar compilador
        result = subprocess.run(
            [COMPILER, infile],
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            # error del compilador (tipo sintaxis)
            return jsonify({"error": result.stderr}), 400

        # cargar assembly
        with open(outfile, "r") as f:
            asm_lines = [line.rstrip("\n") for line in f.readlines()]

        # correr simulador
        steps = run_simulator(asm_lines)

        return jsonify({
            "instructions": asm_lines,
            "steps": steps
        })

    except Exception as e:
        # cualquier error de Python (por ej. no encuentra ../a.out)
        return jsonify({"error": f"Backend exception: {e}"}), 500



# ============================================================
# MAIN
# =============================================================

if __name__ == "__main__":
    app.run(port=5000, debug=True)
