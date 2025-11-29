fn fac(n: i32) -> i32 {
    if (n < 2) {
        return n;
    } else {
        return fac(n - 1) * n;
    }
}

fn main() -> i32 {
    let mut x: i32 = 0;
    x = 1;
    while (x < 20) {
        println!("{}", fac(x));
        x = x + 1;
    }
    return 0;
}
