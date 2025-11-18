struct Point {
    x: i64,
    y: i64,
}

struct Line {
    from: Point,
    to: Point,
}

fn main() {
    let l: Line = Line {
        from: Point { x: 0,  y: 0  },
        to:   Point { x: 10, y: 10 }
    };
    println!("{}", l.from.x);
    println!("{}" , l.to.y);
    return(0)
}
