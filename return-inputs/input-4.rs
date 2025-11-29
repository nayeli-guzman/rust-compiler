fn build() -> [i64; 2] {
    let mut a: i64 = 30;
    let mut b: i64 = 12;

    let mut min: i64 = a;
    let mut max: i64 = a;

    if (b < a) {
        min = b;
    } else {
        max = b;
    }

    return([min, max]);
}

fn main() {
    let arr: [i64; 2] = build();
    println!("{}", arr[0]);
    println!("{}", arr[1]);
}
