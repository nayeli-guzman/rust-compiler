use std::ops::Sub;

struct Punto {
    x: i32,
    y: i32,
}

// Implementación del operador '-'
impl Sub for Punto {
    type Output = Punto;

    fn sub(self, other: Punto) -> Punto {
        Punto {
            x: self.x - other.x,
            y: self.y - other.y,
        }
    }
}

fn main() {
    let p1 = Punto { x: 10, y: 5 };
    let p2 = Punto { x: 3,  y: 1 };

    let p3 = p1 - p2;

    println!("({}, {})", p3.x, p3.y);
}
