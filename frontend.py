import streamlit as st
import requests
import json
import pandas as pd

st.set_page_config(
    page_title="Rust Compiler",
    layout="wide",
    page_icon="🦀"
)

# ======== ESTILO PERSONALIZADO: FONDO CON IMAGEN ========
page_bg_img = """
<style>
[data-testid="stAppViewContainer"] {
    background-image: url("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQW8lE_r0ga9_rj2V5Z4pXmwxhNQxCjOvsJbw&s");
    background-size: cover;
    background-position: center;
    background-repeat: no-repeat;
}

[data-testid="stHeader"] {
    background: rgba(0,0,0,0);
}

[data-testid="stSidebar"] {
    background-color: rgba(30, 30, 30, 0.7);
}
</style>
"""

st.markdown(page_bg_img, unsafe_allow_html=True)

panel_style = """
<style>
.block-container {
    background: rgba(0,0,0,0.55);
    border-radius: 12px;
    padding: 20px;
}
</style>
"""
st.markdown(panel_style, unsafe_allow_html=True)


# -----------------------
# Función: convertir registros hex → decimal
# -----------------------
def format_reg_hex_to_dec(regs):
    formatted = {}
    for r, v in regs.items():
        try:
            dec = int(v, 16)
            formatted[r] = f"{dec}  ({v})"   # "3 (0x00003)"
        except:
            formatted[r] = v
    return formatted


# -----------------------
# UI
# -----------------------

st.markdown("""
# 🔧 Rust Compiler
Ingresar un input válido del proyecto en Rust.
""")

code_input = st.text_area(
    "Código:",
    height=220,
    value="""fn main() {
    let x: i64 = 2;
    let y: i64 = 3;
    return x + y;
}"""
)

if st.button("Compilar"):
    with st.spinner("Compilando y ejecutando simulador…"):
        try:
            r = requests.post(
                "http://127.0.0.1:5000/compile",
                json={"code": code_input},
                timeout=10
            )

            # 1) si no es 200, intento leer mensaje de error
            if r.status_code != 200:
                try:
                    err = r.json().get("error", r.text)
                except Exception:
                    err = r.text
                st.error(f"Backend respondió {r.status_code}:\n{err}")
                st.stop()

            # 2) ahora sí parseo JSON normal
            data = r.json()

        except Exception as e:
            st.error(f"Error al comunicar con el backend: {e}")
            st.stop()

    st.session_state["instructions"] = data.get("instructions", [])
    st.session_state["steps"] = data.get("steps", [])


# Verificar si hay resultados
if "steps" not in st.session_state:
    st.stop()

instructions = st.session_state["instructions"]
steps = st.session_state["steps"]

# -----------------------
# LAYOUT PRINCIPAL
# -----------------------
col_code, col_sim = st.columns([1, 2])

# ============================================================
# PANEL IZQUIERDO: CÓDIGO ASSEMBLY COMPLETO
# ============================================================
with col_code:
    st.subheader("📄 Código Assembly generado")

    if instructions:
        st.code("\n".join(instructions), language="asm")
    else:
        st.warning("Todavía no se ha generado assembly.")


# ============================================================
# PANEL DERECHO: SIMULADOR PASO A PASO
# ============================================================
with col_sim:
    st.subheader("🔍 Visualización instrucción por instrucción")

    step = st.slider(
        "Paso:",
        0,
        len(steps) - 1,
        0,
        key="step_slider"
    )

    current = steps[step]

    # ------------------------
    # Instrucción actual
    # ------------------------
    st.markdown("### 🟦 Instrucción actual")
    st.code(current["instruction"], language="asm")

    # ============================================================
    # REGISTROS Y PILA LADO A LADO
    # ============================================================
    col_r, col_s = st.columns([1, 1])

    # --------- REGISTROS ---------
    with col_r:
        st.markdown("### 🧠 Registros")

        regs_fmt = [
            {
                "Registro": reg,
                "Decimal": int(val, 16),
                "Hex": val
            }
            for reg, val in current["registers"].items()
        ]

        st.table(regs_fmt)

    # --------- PILA ---------
    with col_s:
        st.markdown("### 📦 Pila")

        stack_fmt = [
            {
                "Dirección": cell["address"],
                "Valor (decimal)": int(cell["value"], 16)
            }
            for cell in current["stack"]
        ]

        st.table(stack_fmt)


