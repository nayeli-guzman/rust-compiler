fn cond(x: i32) -> i32 {
    if x < 4 {
        return 0;
    } else {
        return 1;
    }
}

fn main() -> i32 {
    println!("{}", cond(5));
    println!("{}", cond(3));
    return 0;
}
