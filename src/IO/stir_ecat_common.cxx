//
// $Id$
//
/*!
  \file
  \ingroup ECAT
  \ingroup IO

  \brief Implementation of routines which convert ECAT6 and ECAT7 things into our 
  building blocks and vice versa. 

  \author Kris Thielemans
  \author PARAPET project

  $Date$
  $Revision$
*/
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000- $Date$, IRSL
    See STIR/LICENSE.txt for details
*/

#include "stir/ByteOrder.h"
#include "stir/NumericType.h"
#include "stir/Scanner.h" 
#include "stir/IO/stir_ecat_common.h"

START_NAMESPACE_STIR


void find_type_from_ECAT_data_type(NumericType& type, ByteOrder& byte_order, const short data_type)
{
  switch(data_type)
  {
  case ECAT_Byte_data_type:
    type = NumericType("signed integer", 1);
    byte_order=ByteOrder::little_endian;
    return;
  case ECAT_I2_little_endian_data_type:
    type = NumericType("signed integer", 2);
    byte_order=ByteOrder::little_endian;
    return;
  case ECAT_I2_big_endian_data_type:
    type = NumericType("signed integer", 2);
    byte_order = ByteOrder::big_endian;
    return;
  case ECAT_R4_VAX_data_type:
    type = NumericType("float", 4);
    byte_order=ByteOrder::little_endian;
    return;
  case ECAT_R4_IEEE_big_endian_data_type:
    type = NumericType("float", 4);
    byte_order=ByteOrder::big_endian;
    return;
  case ECAT_I4_little_endian_data_type:
    type = NumericType("signed integer", 4);
    byte_order=ByteOrder::little_endian;
    return;
  case ECAT_I4_big_endian_data_type:
    type = NumericType("signed integer", 4);
    byte_order=ByteOrder::big_endian;
    return;    
  default:
    error("find_type_from_ecat_data_type: unsupported data_type: %d", data_type);
    // just to avoid compiler warnings
    return;
  }
}

short find_ECAT_data_type(const NumericType& type, const ByteOrder& byte_order)
{
  if (!type.signed_type())
    warning("find_ecat_data_type: ecat data support only signed types. Using the signed equivalent\n");
  if (type.integer_type())
  {
    switch(type.size_in_bytes())
    {
    case 1:
      return ECAT_Byte_data_type;
    case 2:
      return byte_order==ByteOrder::big_endian ? ECAT_I2_big_endian_data_type : ECAT_I2_little_endian_data_type;
    case 4:
      return byte_order==ByteOrder::big_endian ? ECAT_I4_big_endian_data_type : ECAT_I4_little_endian_data_type;
    default:
      {
        // write error message below
      }
    }
  }
  else
  {
    switch(type.size_in_bytes())
    {
    case 4:
      return byte_order==ByteOrder::big_endian ? ECAT_R4_IEEE_big_endian_data_type : ECAT_R4_VAX_data_type;
    default:
      {
        // write error message below
      }
    }
  }
  string number_format;
  size_t size_in_bytes;
  type.get_Interfile_info(number_format, size_in_bytes);
  error("find_ecat_data_type: ecat does not support data type '%s' of %d bytes.\n",
    number_format.c_str(), size_in_bytes);
  // just to satisfy compilers
  return short(0);
}

short find_ECAT_system_type(const Scanner& scanner)
{
  switch(scanner.get_type())
  {
  case Scanner::E921:
    return 921; 
    
  case Scanner::E931:
    return 931; 
    
  case Scanner::E951:
    return 951; 
    
  case Scanner::E953:
    return 953;

  case Scanner::E961:
    return 961;

  case Scanner::E962:
    return 962; 
    
  case Scanner::E966:
    return 966;

  case Scanner::RPT:
    return 128;

  case Scanner::RATPET:
    return 42;

  default:
    warning("\nfind_ecat_system_type: scanner \"%s\" currently unsupported. Returning 0.\n", 
      scanner.get_name().c_str());
    return 0;
  }
}

END_NAMESPACE_STIR
