struct Punto {
    x: i64,
    y: i64
}

impl Add for Punto {
    fn add(self: Punto, other: Punto) -> Punto {
        Punto { x: self.x + other.x, y: self.y + other.y }
    }
}

fn main() {
    let p1: Punto = Punto { x: 1, y: 2 };
    let p2: Punto = Punto { x: 3, y: 4 };
    let p3: Punto = add(p1, p2);
    println!("{}", p3.x);
    println!("{}", p3.y);
}
