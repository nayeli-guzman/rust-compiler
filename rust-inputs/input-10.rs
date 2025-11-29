fn cond(x: i64) -> i64 {
    if (x < 4) {
        return 0;
    } else {
        return 1;
    }
}

fn main() -> i64 {
    println!("{}", cond(5));
    println!("{}", cond(3));
    return 0;
}
