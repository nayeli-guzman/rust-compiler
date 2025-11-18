struct Counter {
    value: i64,
}

fn main() {
    let mut c: Counter = Counter { value: 0 };

    while c.value < 5 {
        println!("{}", c.value);
        c.value = c.value + 1;
    }

    if c.value < 10 {
        println!("{}", 100);
    } else {
        println!("{}", 200);
    }

    return(0)
}
