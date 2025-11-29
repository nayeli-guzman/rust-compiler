struct Punto {
    x: i64,
    y: i64,
}

fn main() {
    let mut p: Punto = Punto { x: 0, y: 0 };

    p.x = 10 + 0 + 0 + 0;
    p.y = 0 + 20;

    println!("{}", p.x + 1);
    println!("{}", p.y + 0);

    return(0);
}
