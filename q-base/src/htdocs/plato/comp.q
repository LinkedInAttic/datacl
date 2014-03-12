q start_compound
q f1s1opf2 TM CountryId 30:31:2 == "x'" 
q f1s1opf2 TM IndustryId 12:92 != "y'"
q f1f2opf3 TM "x'" "y'" '&&' "z'" 
q f1s1opf2 TM TenureBucketId 3 == "w'"
q f1f2opf3 TM "w'" "z'" '&&' "v'"
q count TM CountryId "v'" TCountry cnt 
q stop_compound "" ""
