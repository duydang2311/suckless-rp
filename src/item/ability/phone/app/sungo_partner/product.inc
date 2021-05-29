#if defined sungo_partner_product_included
	#endinput
#endif

#define sungo_partner_product_included

const SGO_PRODUCT_NAME_SIZE = 32;

enum SGoProduct {
	ProductName[SGO_PRODUCT_NAME_SIZE],
	ProductPrice
}