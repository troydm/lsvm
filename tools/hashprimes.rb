#!/usr/bin/env ruby

"Ruby script used to calculate prime numbers up to 1,6GB for hash table"

def is_prime(n)
    h = n/2+1
    i = 2
    while n % i != 0 and h > i
        i += 1
    end
    i == h
end


print '{'
c = 0
i = 4
while c < 24
    l = 2 ** i 
    u = l*2
    m = (u - l)/2 + l
    while m < u    
        if is_prime(m)
            c += 1
            print m
            print ','
            break
        else
            m += 1
        end
    end
    i += 1
end
puts '}'
puts 'count: ' + c.to_s
