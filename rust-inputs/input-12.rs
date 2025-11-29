fn signo(x: i64) -> i64 {
    if (x < 5) {
        return 1;
    } else {
        return 0;
    }
}

fn main() -> i64 {
    println!("{}", signo(3));
    println!("{}", signo(0));
    println!("{}", signo(10));
    return 0;
}
