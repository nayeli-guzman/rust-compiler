fn fib(n: i64) -> i64 {
    if (n < 2) {
        return n;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

fn main() -> i64 {
    let mut x: i64 = 0;
    x = 1;
    while (x < 50) {
        println!("{}", x);
        println!("{}", fib(x));
        x = x + 1;
    }
    return 0;
}
