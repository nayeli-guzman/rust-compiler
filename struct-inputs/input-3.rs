struct Point {
    x: i64,
    y: i64,
}

fn main() -> i64 {
    let mut p: Point = Point { x: 10, y: 20 };
    println!("{}", p.x);
    println!("{}", p.y);
    return(0)
}
