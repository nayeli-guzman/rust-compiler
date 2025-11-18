fn fib(n: i32) -> i32 {
    if n < 2 {
        return n;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

fn main() -> i32 {
    let mut x: i32 = 0;
    x = 1;
    while x < 50 {
        println!("{}", x);
        println!("{}", fib(x));
        x = x + 1;
    }
    return 0;
}
