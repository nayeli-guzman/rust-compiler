struct Punto { x: i64, y: i64 }

fn restar(a: Punto, b: Punto) -> Punto {
    return(Punto { x: a.x - b.x, y: a.y - b.y });
}

fn main() {
    let p1: Punto = Punto { x: 10, y: 20 };
    let p2: Punto = Punto { x: 3,  y: 7  };

    let p3: Punto = restar(p1, p2);

    println!("{}", p3.x);
    println!("{}", p3.y);
}
