use std::ops::Add;
use std::ops::Sub;

struct Punto { x: i32, y: i32 }

impl Add for Punto {
    type Output = Punto;
    fn add(self, other: Punto) -> Punto {
        let ans = Punto { 
            x: self.x + other.x, 
            y: self.y + other.y 
        };
        return ans;
    }
}

impl Sub for Punto {
    type Output = Punto;
    fn sub(self, other: Punto) -> Punto {
        let ans = Punto { 
            x: self.x - other.x, 
            y: self.y - other.y 
        };
        return ans;
    }
}

fn main() {
    let p1 = Punto { x: 1, y: 2 };
    let p2 = Punto { x: 3, y: 4 };
    let p3 = p1 + p2;
    let p4 = p3 - p1;
    println!("{}", p3.x);
    println!("{}", p3.y);
    println!("{}", p4.x);
    println!("{}", p4.y);
}