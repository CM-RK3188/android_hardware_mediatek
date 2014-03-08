#!/bin/sh

function brace_parser()
{
#echo "brace_parser() start, $1"
i=1 ; j=1 ; ln=1

if [ -s $1 ] ; then
while read line
do
{
  if [[ ${line} == "{" ]] ; then
      a[${i}]=$ln
      ((i++))
  elif [[ ${line} == "}" ]] ; then
      b[${j}]=$ln
      ((j++))
  fi

  ((ln++))
}
done < $1

if [ ${#a[@]} -ne ${#b[@]} ] ; then
    echo "Error: the brace is not match in config file: $1 !"
    echo "Error: " ${#a[@]} "{ ," ${#b[@]} "}"
    exit 1
fi

else
    echo "Error: the file: $PWD/$1 is not found!"
    exit 1
fi
#echo "brace_parser() end, $1"
}


function do_checklist()
{
#echo "do_checklist() start, $1"

if [ -s $1 ] ; then
#echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
i=1; ln=1

while [ ${i} -le ${#a[@]} ]
do
{
    if [ ${ln} -gt ${a[i]} ] && [ ${ln} -lt ${b[i]} ] ; then
       #Only deal the statement within "{ }":"
       if [ ${i} -ne 1 ] ; then
          #Only export the paramenter in the 1st block "ENVIROMENT_SET:"
#          export `sed -n ''${ln}p'' $1`
      
#      else
          #Set the path for the other block "MODULE_***" : 
          Temp=`sed -n ''${ln}p'' $1`
          
          #Not deal the statements :only 'space',  or marked by '#' 
          Temp1=${Temp// /}
          if [[ ${Temp1} == "" ]] || [[ ${Temp1:0:1} == "#" ]] ; then
              ((ln++))
              continue
          fi
          MIDPATH=`eval echo ${Temp}`
#          CHECKPATH=${BASE_PATH}/${MIDPATH}
          CHECKPATH=${MIDPATH}
#          echo "CHECKPATH=${CHECKPATH}" >> $RESULT_FILE
          #Check the file existence in the block "MODULE_***" : 
          if [ ! -s ${CHECKPATH} ] ; then
#            echo "Error: The last file is not exist!" >> $RESULT_FILE
	    echo "CHECKPATH=${CHECKPATH} : Not Exist" >> $RESULT_FILE
	    ((error_times++))
          fi
       fi

    elif [ ${ln} -gt ${b[i]} ] ; then
       #Not deal the statement without "{}":"
       ((i++))

       if [ ${i} -le ${#a[@]} ] ; then
          ln=${a[i]}
       fi
       continue
    fi

    ((ln++))
}
done
#echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

#    echo "Print some paraments in block 'ENVIROMENT_SET:' in $1"
#    echo "BASE_PATH: ${BASE_PATH}"
#    echo "RPODUCT_OUT_PATH: ${RPODUCT_OUT_PATH}"
#    echo "XBIN_PATH: ${XBIN_PATH}"
#    echo "BTN_PATH:  ${BTN_PATH}"
#    echo ""
else
    echo "Error: the file: $PWD/$1 is not found!"
    exit 1
fi
#echo "do_checklist() end, $1"
}

#echo ""
#echo "********0.[Begin] Auto Check Product library************"

#echo "The number of '{' = ${#a[@]} ,   The line '{' exist= ${a[*]}"
#echo "The number of '}' = ${#b[@]} ,   The line '}' exist= ${b[*]}"
if [ $# -lt 1 ]; then
PRODUCT_NAME=`export | grep TARGET_PRODUCT | awk -F '\"' '{print $2}' | sed 's/^[a-z A-Z 0-9]*_//'`
else
PRODUCT_NAME=$1
fi

if  [[ ${PRODUCT_NAME} == "" ]] && [[ $1 == "" ]] ; then
    echo "Usage: $0 [product_name]"
    exit -1
fi

CHECK_LIST=./checklist_MT6620_common.dat
RPODUCT_OUT_PATH=../../../../out/target/product/$PRODUCT_NAME/system
RESULT_FILE=./result.txt
export RPODUCT_OUT_PATH=../../../../out/target/product/$PRODUCT_NAME/system

rm -rf $RESULT_FILE

error_times=0

echo ""
echo "Check list      : [$CHECK_LIST]"
echo "Product out path: [$RPODUCT_OUT_PATH]"
echo "Result File     : [$RESULT_FILE]"

brace_parser $CHECK_LIST

do_checklist $CHECK_LIST

echo "============================================"
if [ $error_times -ne 0 ]; then
echo "Check Error:"
echo "Please see result file in : [$RESULT_FILE]"
else
echo "Check Success!"
fi
echo "============================================"
echo ""
#echo "********0.[End]   Auto Check Product library************"




