fn fib(n: i64) -> i64 {
    if (n < 2) {
        return n;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

fn main() -> i64 {
    println!("{}", fib(10));
    return 0;
}
