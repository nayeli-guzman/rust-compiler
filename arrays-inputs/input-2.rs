struct Punto {
    x: i64,
    y: i64,
}

fn main() {

    let mut puntos: [Punto; 3] = [
        Punto { x: 10, y: 20 },
        Punto { x: 30, y: 40 },
        Punto { x: 50, y: 60 }
    ];


    println!("{}", puntos[0].x); 
    println!("{}", puntos[1].y); 

    return(0)
}
