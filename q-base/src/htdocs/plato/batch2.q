q f1opf2 TCountry cnt 'op=[conv]:newtype=[I4]' cnt
q dup_fld TCountry idx xidx 
q sortf1f2 TCountry cnt xidx D_ 
q set_meta TCountry xidx dict_tbl TCountry 
q pr_fld TCountry xidx:cnt 'lb=[0]:ub=[5]' 
q delete TM x:y 
q delete TCountry cnt:xidx 
q delete tempt
