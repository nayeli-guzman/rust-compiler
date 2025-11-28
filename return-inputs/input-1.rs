fn build() -> [i64; 2] {
    let mut x: i64 = 10;
    let mut y: i64 = 20;
    return([x, y]);
}

fn main() {
    let arr: [i64; 2] = build();
    println!("{}", arr[0]);
    println!("{}", arr[1]);
}
