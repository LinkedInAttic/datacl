set -e
mode=$1

rm -f _x
q describe TB 2>/dev/null | grep ",n_" | cut -f 2 -d ","  >> _x
q describe TB 2>/dev/null | grep ",c_" | cut -f 2 -d ","  >> _x
q describe TB 2>/dev/null | grep ",l_" | cut -f 2 -d ","  >> _x
q describe TB 2>/dev/null | grep ",cumx_" | cut -f 2 -d ","  >> _x
q describe TB 2>/dev/null | grep ",xcl_" | cut -f 2 -d ","  >> _x
q describe TB 2>/dev/null | grep ",x_" | cut -f 2 -d ","  >> _x
q describe TB 2>/dev/null | grep ",b_" | cut -f 2 -d ","  >> _x

if [ "$mode" == "chk" ]; then 
  echo "Following fields should not exist"
  cat _x
  echo "Above     fields should not exist"
else 
  cat _x | sed s'/^/q delete TB /'g > _y
  echo "Deleting following fields";
  echo "Deleting above      fields";
  cat _y
  bash _y
fi



