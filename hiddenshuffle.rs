// Tested with Rust 1.0 which lacks random number generation in the standard library.
// Ensure that the crate "rand" is installed ("cargo install rand").
// Create a new project with ("cargo new hiddenshuffle").
// Add rand as a dependency ("cargo add rand").
// Put this code into the main file.
// For runtime measurements always use release-mode ("cargo run --release").
// The Cargo.toml file should look similar to the following:
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
use rand::seq::SliceRandom;

struct HiddenShuffle<'a>{
    H: usize, L: usize, N: usize, n: usize, a: f64,
    rng : &'a mut rand::rngs::ThreadRng
}

impl <'a> Iterator for HiddenShuffle<'a>{

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
			
			let pi = 1.0-1.0*f64::from( (N-n) as u32)/f64::from( (N-i).max(1) as u32);
		
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
	
	const REPS : usize = 250;
	const POPULATION : usize = 1000*1000*10;
	const SAMPLESIZE : usize = 1000*1000;
	
	let mut sample: [usize; SAMPLESIZE] = [0; SAMPLESIZE];
	
	println!("\nGetting a sorted sample with hidden shuffle using O(1) memory");
	println!("and a shuffled sample using O(k) memory is comparably fast:");
	
	{
		let now = Instant::now(); let mut sum : usize = 0;
	
		for i in 0..REPS{
	
			let mut k = 0;
			for j in rand::seq::index::sample(&mut rng, POPULATION,SAMPLESIZE){
				sum += i+j; k = k+1;
			}
			assert!(k == SAMPLESIZE);
		}
	
		println!("\nchecksum = {} (ignore this)", sum);
		println!("It takes {} seconds for rand::seq::index::sample to get {} shuffled sets of {} numbers between 0 and {}.", now.elapsed().as_secs(), REPS, SAMPLESIZE, POPULATION-1);
	}
	
	{
		let now = Instant::now(); let mut sum : usize = 0;
		
		for i in 0..REPS{
		
			let mut k = 0;
			for j in seqsample(&mut rng, POPULATION, SAMPLESIZE) {
				sum += i+j; k = k+1;
			}
			assert!(k == SAMPLESIZE);
		}
		println!("\nchecksum = {} (ignore this)", sum);
		println!("It takes {} seconds for hiddenshuffle to get {} sorted sets of {} numbers between 0 and {}.", now.elapsed().as_secs(), REPS, SAMPLESIZE, POPULATION-1);
	}
	
	println!("\nSorted sample+shuffling is slower than getting a shuffled sample directly:");
	
	{
		let now = Instant::now(); let mut sum : usize = 0;
		
		for i in 0..REPS{
	
			let mut k = 0;
			for j in seqsample(&mut rng, POPULATION, SAMPLESIZE){
			
				sample[k] = j; k = k+1; sum += i+j;
			}
			assert!(k == SAMPLESIZE); sample.shuffle(&mut rng); 
		}
		println!("\nchecksum = {} (ignore this)", sum);
		println!("It takes {} seconds for hiddenshuffle + shuffling to get {} shuffled sets of {} numbers between 0 and {}.", now.elapsed().as_secs(), REPS, SAMPLESIZE, POPULATION-1);
	}
	
	println!("\nShuffled sample+sorting is noticeably slower than getting a sorted sample directly:");
	
	{
		let now = Instant::now(); let mut sum : usize = 0;
		
	
		for i in 0..REPS{
	
			let mut k = 0;
			for j in rand::seq::index::sample(&mut rng, POPULATION, SAMPLESIZE){
		
				sample[k] = j; k = k+1; sum += i+j;
			}
			assert!(k == SAMPLESIZE); sample.sort();
		}
		println!("\nchecksum = {} (ignore this)", sum);
		println!("It takes {} seconds for rand::seq::index::sample + sorting to get {} sorted sets of {} numbers between 0 and {}.", now.elapsed().as_secs(), REPS, SAMPLESIZE, POPULATION-1);
	}
}
