set -e 
q f1s1opf2 TM CountryId 30:31:2 == x 
q f1s1opf2 TM IndustryId 12:92 != y 
q f1f2opf3 TM x y '&&' x 
q f1s1opf2 TM TenureBucketId 3 == y 
q f1f2opf3 TM x y '&&' x 
q f1f2_to_s TM x MemberId sum
