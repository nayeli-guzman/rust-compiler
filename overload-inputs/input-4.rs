use std::ops::Div;
use std::ops::Mul;

struct Punto { x: i32, y: i32 }
struct Objeto { x: i32, y: i32 }

impl Div for Punto {
    type Output = Punto;
    fn div(self, other: Punto) -> Punto {
        let ans = Punto { 
            x: self.x / other.x, 
            y: self.y / other.y 
        };
        return ans;
    }
}

fn main() {
    let p1 = Punto { x: 1, y: 2 };
    let p2 = Punto { x: 3, y: 4 };
    let p3 = p1 / p2;
    let variable = 6 / 2;
    println!("{}", p3.x);
    println!("{}", p3.y);
}