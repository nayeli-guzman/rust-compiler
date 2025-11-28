struct Mensaje {
    texto: String,
    veces: i64
}

fn main() {
    let mut msg: Mensaje = Mensaje {
        texto: "hola desde struct",
        veces: 3
    };

    println!("{}", "---- Programa 3 ----");
    println!("{}", msg.texto);
    println!("{}", msg.veces);

    msg.texto = "texto actualizado";

    println!("{}", msg.texto);

    return(0)
}
