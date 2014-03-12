set -e 
q start_compound
q f1s1opf2 TM CountryId 30:31:2 == "x'"
q f1s1opf2 TM IndustryId 12:92 != "y'" 
q f1f2opf3 TM "x'" "y'" '&&' "z'" 
q f1s1opf2 TM TenureBucketId 3 == "w'"
q f1f2opf3 TM "w'" "z'" '&&' "v'"
q count TM CountryId "v'" TCountry cnt 
q stop_compound "" ""
q f1opf2 TCountry cnt 'op=[conv]:newtype=[I4]' cnt
q dup_fld TCountry idx xidx 
q sortf1f2 TCountry cnt xidx D_ 
q set_meta TCountry xidx dict_tbl TCountry 
q pr_fld TCountry xidx:cnt 'lb=[0]:ub=[5]' 
q delete TM x:y 
q delete TCountry cnt:xidx 
q delete tempt
