struct Persona {
    nombre: String,
    edad: i64
}

fn saludar(nombre: String) {
    println!("{}", "---- saludo ----");
    println!("{}", nombre);
    return(0);
}

fn main() {
    let mut personas: [Persona; 2] = [
        Persona { nombre: "Ana", edad: 20 },
        Persona { nombre: "Luis", edad: 25 }
    ];

    println!("{}", "---- Programa 5 ----");
    println!("{}", personas[0].nombre);
    println!("{}", personas[0].edad);
    println!("{}", personas[1].nombre);
    println!("{}", personas[1].edad);

    saludar("desde funcion");
    saludar(personas[0].nombre);

    return(0)
}
