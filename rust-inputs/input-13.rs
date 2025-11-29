fn fac(n: i64) -> i64 {
    if (n < 2) {
        return n;
    } else {
        return fac(n - 1) * n;
    }
}

fn main() -> i64 {
    let mut x: i64 = 0;
    x = 1;
    while (x < 20) {
        println!("{}", fac(x));
        x = x + 1;
    }
    return 0;
}
