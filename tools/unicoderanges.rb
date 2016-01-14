#!/usr/bin/env ruby
require 'nokogiri'
require 'open-uri'

def parse(url)
    doc = Nokogiri::HTML(open(url))
    ucs = []
    doc.xpath("//tr[@class=\"row0\"] | //tr[@class=\"row1\"]").each do |row0|
          unicode = row0.xpath("td")[0].content
          unicode = unicode.slice(2,unicode.size-2)
          dec = unicode.hex
          ucs.push(dec)
    end

    s = 0
    sc = 0
    ucs.each_index do |i|
        c = ucs[i]
        if s == 0
            s = c
            sc = 1
        else
            if ucs[i-1] == (c-1)
                sc += 1
            else
                if sc > 1
                    puts "0x"+s.to_s(16).upcase+"-0x"+(s+sc-1).to_s(16).upcase
                end
                s = c
                sc = 1
            end
        end 
    end
end

parse 'http://www.fileformat.info/info/unicode/category/Lu/list.htm'
