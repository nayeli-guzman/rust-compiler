use std::ops::Div;
use std::ops::Mul;

struct Punto { x: i64, y: i64 }
struct Objeto { x: i64, y: i64 }

impl Div for Punto {
    type Output = Punto;
    fn div(self, other: Punto) -> Punto {
        let ans: Punto = Punto { 
            x: self.x / other.x, 
            y: self.y / other.y 
        };
        return ans;
    }
}

fn main() {
    let p1: Punto = Punto { x: 1, y: 2 };
    let p2: Punto = Punto { x: 3, y: 4 };
    let p3: Punto = Punto { x: 0, y: 0 };
    let variable: i64 = 6 / 2;

    p3 = p1 / p2;
    println!("{}", p3.x);
    println!("{}", p3.y);
    println!("{}", variable);
}