// Example for Cargo.toml given in next comment):
/*
[package]
name = "rust"
version = "0.1.0"
edition = "2021"

# See more keys and their definitions at https://doc.rust-lang.org/cargo/reference/manifest.html

[dependencies]
rand = "0.8.5"
*/

extern crate rand;
use rand::{thread_rng,Rng}; use std::time::{Instant};

struct HiddenShuffle <'x>{
    H: usize, L: usize, N: usize, n: usize, a: f64,
    rng : &'x mut rand::rngs::ThreadRng
}

impl Iterator for HiddenShuffle<'_> {

	type Item = usize;
	fn next(&mut self) -> Option<Self::Item> {
	   
	   while self.H > 0{
	
			let S_old = self.n+(self.a*f64::from( (self.N-self.n) as u32).floor()) as usize;
			self.a *= self.rng.gen::<f64>().powf(1.0/f64::from(self.H as u32));
			let S = self.n+(self.a*f64::from( (self.N-self.n) as u32 ).floor()) as usize;
		
			if S < S_old{
				self.H -= 1;
				return Some((self.N-1)-S);
			
			} else {
		
				self.L += 1; // duplicate detected
				self.H -= 1;
			}
		}
		
		while self.L > 0{
		
			let u = self.rng.gen::<f64>();
			let mut s = 0;
			let mut F = f64::from(self.L as u32)/f64::from(self.n as u32);
			
			while F < u && s < (self.n-self.L){
			
				F = 1.0-(1.0-f64::from(self.L as u32)/f64::from( (self.n-s-1) as u32))*(1.0-F);
				s += 1;
			}
		
			self.L -= 1;
			self.n = self.n-s-1;
			
			return Some((self.N-1)-self.n);
		}
	
		return None
    }
}

fn seqsample(rng : & mut rand::rngs::ThreadRng, N_ : usize, n_ : usize) -> HiddenShuffle {
	
	let N = N_; let n = n_;
	let mut H : usize = 0; let mut i : usize = 0;
	
	if N > n {
	
		H = n;
		while i < n{
	
			let q = 1.0-1.0*f64::from( (N-n) as u32)/f64::from( (N-i) as u32);
		
			i += rng.gen::<f64>().log(1.0-q).floor() as usize;
			
			let pi = 1.0-1.0*f64::from( (N-n) as u32)/f64::from( (N-i) as u32);
		
			if i < n && (rng.gen::<f64>() < (pi/q) ){
				H -= 1;
			}
			i += 1;
		}
	}
	
    HiddenShuffle { N:N, n:n, H: H, L: n-H, a: 1.0, rng: rng}
}

fn main() {
	
	let mut rng = thread_rng();
	
	const REPS : usize = 10;
	const POPULATION : usize = 1000*1000*10;
	const SAMPLESIZE : usize = 1000*1000;
	
	{
		let now = Instant::now();
		let mut sum : usize = 0;
	
		for i in 1..REPS{
	
			for j in rand::seq::index::sample(&mut rng, POPULATION,SAMPLESIZE){
		
				sum += i+j;
			}
		}
	
		println!("checksum = {}\n", sum);
		println!("rand::seq::index::sample takes {} seconds for {} shuffled sets of {} numbers between 0 and {}", now.elapsed().as_secs(), REPS, SAMPLESIZE, POPULATION-1);
	}
	
	{
		let now = Instant::now();
		let mut sum : usize = 0;
		
		for i in 1..REPS{
		
			for j in seqsample(&mut rng, POPULATION, SAMPLESIZE) {
				sum += i+j
			}
		}
		println!("checksum = {}\n", sum);
		println!("hiddenshuffle takes {} seconds for {} sequential sets of {} numbers between 0 and {}", now.elapsed().as_secs(), REPS, SAMPLESIZE, POPULATION-1);
	}
	
	println!("\n\nNote: this indicates one generate a sequential/sorted sample as quickly as a shuffled one");
	println!("while reducing the memory footprint to O(1)");
}
