struct Point {
    x: i64,
    y: i64,
}

fn main() -> i64 {
    let mut p: Point = Point { x: 0, y: 0 } ;
    let mut x: i64 = 5;
    
    p = Point { x: 10, y: 20 } ;
    x = 1;

    println!("{}", p.x);
    println!("{}", p.y);
    return(0)
}
