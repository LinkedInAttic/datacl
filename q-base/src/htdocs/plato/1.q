set -e 
q f1s1opf2 TM CountryId 30:31:2 == x 
q f1s1opf2 TM HasLanguages 1 != y 
q f1f2opf3 TM x y '&&' x 
q f1s1opf2 TM HasPositions 1 == y 
q f1f2opf3 TM x y '&&' x 
q count TM CountryId 'x' TCountry cnt 
q f1opf2 TCountry cnt 'op=[conv]:newtype=[I4]' cnt
q dup_fld TCountry idx xidx 
q sortf1f2 TCountry cnt xidx D_ 
q set_meta TCountry xidx dict_tbl TCountry 
q pr_fld TCountry xidx:cnt 'lb=[0]:ub=[10]' 
q delete TM x:y 
q delete TCountry cnt:xidx 
q delete tempt
