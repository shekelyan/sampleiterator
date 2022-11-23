package main

import (
    "fmt"
    "math/rand"
    "math"
    "time"
)

func HiddenShuffle(N int, n int) <- chan int {
    
    y := make(chan int)

    go func() {
    	
        H := 0; i := 0
        if N > n {
        
        	H = n
        	for ; i < n ; {
        	
        		q := 1.0-float64(N-n)/float64(N-i)
        		i = i+int( math.Log(rand.Float64())/math.Log(1.0-q) )
        		p_i := 1.0-float64(N-n)/math.Max(float64(N-i),1.0)
        		if (i < n) && (rand.Float64() < p_i/q) {
        			H = H-1
        		}
        		i = i+1
        	}
        }
        L := n-H; a := 1.0
        for ; H > 0; {
        	
        	S_old := n+int(a*float64(N-n))
        	a = a * math.Pow(rand.Float64(), 1.0/float64(H))
        	S := n+int(a*float64(N-n))
        	if S < S_old{
        		y <- (N-1)-S
        	} else {
        		L = L+1
        	}
        	H = H-1
        }
        
        for ; L > 0; {
        
        	u := rand.Float64(); s := 0; F := float64(L)/float64(n)
        	for ; F < u && s < (n-L);{
        	
        		F = 1.0-(1.0-float64(L)/float64(n-s-1) )*(1.0-F)
        		s = s+1
        	}
        	L = L-1; n = n-s-1
        	y <- (N-1)-n
        }
        
        close(y)
        
    }()

    return y
}

func main() {

	rand.Seed(time.Now().Unix())
	c := HiddenShuffle(1000,10)

	for i := range c {
	
		fmt.Println(i)
	}
}
