/*  gray.c includes functions to transform a bitmap image into grayscale
 **
 ** Copyright (C) 2012 Kyriakos Georgiou
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** 
 *  @file gray.c
 *
 *  @brief This function converts a colored image into grayscale
 *
 * 	The function opens a bitmap image and calculates the
 * 	luminance of each pixel. Then, it sets the red,green and blue
 * 	values of the pixel into the calculated luminance.
 *
 * 	@author Kyriakos Georgiou
 *
 * 	@bug No known bugs
 */

/* Definitions' file inclusion */
#include "def.h"

PUBLIC int gray(char *filename) {

	// Pointers to the input and output files.
	FILE *fp_in, *fp_out;

	// Pointer to the file's info header.
	BITMAPINFOHEADER *info_header;

	// Pointer to the file's file header.
	BITMAPFILEHEADER *file_header;

	// 2d array that will represent the new image.
	PIXEL **image;

	// luminance is the value of each pixel after grayscaling.
	byte luminance;

	// p is an assisting variable to easily bypass padding in the input file.
	byte p;

	// The number of padding bytes in each pixel row.
	dword padding;

	// Indices when we are going through the image's pixels.
	dword i, j, k;

	// The name of the final image file.
	char *out_filename;

	/* Opening input file */
	if ((fp_in = fopen(filename, "rb")) == NULL ) {
		printf("**Error: Unable to open file %s**\n", filename);
		return EXIT_FAILURE;
	}

	/* Allocating memory for the image's file headers */
	if ((file_header = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER)))
			== NULL ) {
		printf("**Error: Not enough memory allocated**\n");
		return EXIT_FAILURE;
	}

	/* Allocating memory for the image's info headers */
	if ((info_header = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER)))
			== NULL ) {
		printf("**Error: Not enough memory allocated**\n");
		return EXIT_FAILURE;
	}

	/* Reading the 2 image's headers with a validation check */
	if ((fread(file_header, sizeof(BITMAPFILEHEADER), 1, fp_in)) < 1) {
		printf("**Error: Information missing from file %s**\n", filename);
		fclose(fp_in);
		return EXIT_FAILURE;
	}
	if ((fread(info_header, sizeof(BITMAPINFOHEADER), 1, fp_in)) < 1) {
		printf("**Error: Information missing from file %s**\n", filename);
		fclose(fp_in);
		return EXIT_FAILURE;
	}

	/* Checking to see if the image is a bitmap image of 24 pixels in depth
	 * and compression free */
	if (file_header->bfType1 != 'B' || file_header->bfType2 != 'M') {
		printf("**Error: %s is not a bitmap file**\n", filename);
		return EXIT_FAILURE;
	}
	if (info_header->biCompression != 0) {
		printf("**Error: File %s is compressed**\n", filename);
		return EXIT_FAILURE;
	}
	if (info_header->biBitCount != 24) {
		printf("**Error: File %s is not a 24-bit image**\n", filename);
		return EXIT_FAILURE;
	}

	/* Dimensions check */
	if (info_header->biWidth <= 0) {
		printf("**Error: File %s has a negative or zero pixel dimension**\n",
				filename);
		return EXIT_FAILURE;
	}
	if (info_header->biHeight <= 0) {
		printf("**Error: File %s has a negative or zero pixel dimension**\n",
				filename);
		return EXIT_FAILURE;
	}

	/* Creating the new file's name */
	if ((out_filename = (char *) malloc(strlen(filename) + 5)) == NULL ) {
		printf("**Error: Not enough memory allocated**\n");
		return EXIT_FAILURE;
	}
	strcpy(out_filename, "new-");
	strcat(out_filename, filename);

	/* Opening the new file */
	if ((fp_out = (fopen(out_filename, "wb"))) == NULL ) {
		printf("**Error: Unable to open file %s**\n", out_filename);
		return EXIT_FAILURE;
	}

	/* Writing the image's headers in the new file with a validation check */
	if ((fwrite(file_header, sizeof(BITMAPFILEHEADER), 1, fp_out)) < 1) {
		printf("**Error: Data omissions while writing in file %s**\n",
				out_filename);
		fclose(fp_out);
		return EXIT_FAILURE;
	}
	if ((fwrite(info_header, sizeof(BITMAPINFOHEADER), 1, fp_out)) < 1) {
		printf("**Error: Data omissions while writing in file %s**\n",
				out_filename);
		fclose(fp_out);
		return EXIT_FAILURE;
	}

	/* Creating a two dimensional array of pixels to represent the image */
	if ((image = (PIXEL **) malloc(sizeof(PIXEL *) * (info_header->biHeight)))
			== NULL ) {
		printf("**Error: Not enough memory allocated**\n");
		return EXIT_FAILURE;
	}

	/* Calculating padding bytes */
	padding = 4 - ((info_header->biWidth * sizeof(PIXEL)) & 3);

	if (padding == 4) {
		padding = 0;
	}

	for (i = 0; i < info_header->biHeight; i++) {
		/* Allocating memory for the image's rows in the image array */
		if ((image[i] = (PIXEL *) malloc(sizeof(PIXEL) * (info_header->biWidth)))
				== NULL ) {
			printf("**Error: Not enough memory allocated**");
			return EXIT_FAILURE;
		}

		/* Reading i-th image's row and validation check */
		if ((fread(image[i], sizeof(PIXEL) * info_header->biWidth, 1, fp_in))
				< 1) {
			printf("**Error: Information missing from file %s**\n", filename);
			fclose(fp_in);
			return EXIT_FAILURE;
		}

		for (j = 0; j < (info_header->biWidth); j++) {
			/* Calculating the luminance of each pixel and setting the pixel's colors
			 * into that luminance */

			// +0.5 for rounding purposes.
			luminance = (image[i][j].red * 0.299 + image[i][j].green * 0.587
					+ image[i][j].blue * 0.114 + 0.5);

			image[i][j].red = luminance;
			image[i][j].green = luminance;
			image[i][j].blue = luminance;
		}

		/* Writing the pixels of the line into the new image */
		if ((fwrite(image[i], sizeof(PIXEL) * info_header->biWidth, 1, fp_out))
				< 1) {
			printf("**Error: Data omissions while writing in file %s**\n",
					out_filename);
			fclose(fp_out);
			return EXIT_FAILURE;
		}

		/* Skipping padding in the input image and placing it in the new image */
		for (k = 0; k < padding; k++) {
			p = fgetc(fp_in);
			fputc(0x00, fp_out);
		}
	}

	/* Close input and output files */
	fclose(fp_in);
	fclose(fp_out);
	for (i = 0; i < info_header->biHeight; i++) {
		free(image[i]);
	}

	/* Free memory allocated previously */
	free(image);
	free(info_header);
	free(file_header);
	free(out_filename);

	return EXIT_SUCCESS;
}

/*********************************************************************
 *						 DRIVER'S DOCUMENTATION
 * Function: openFile
 * 	This function sends a non-existing file name into gray() function
 * 	to test if the file - opening check is working properly.
 *
 * Function: readFile
 * 	This function creates an empty file and sends it to gray() function
 * 	to test if the file - reading checks are working properly.
 *
 * Function: isBm
 * 	This function creates a new image with type OK and sends it to gray()
 * 	function to test if the image - checking is working properly.
 *
 * Function: hasCompression
 * 	This function creates a new image with compression set to 1 and sends it
 * 	to gray () function to test if the image - checking is working properly
 *
 * Function: isPixel24Bit
 * 	This function creates a new image with a color depth different than 24
 * 	and sends it to gray() function to test if the image - checking is working
 * 	properly.
 *
 * Function: isWorking
 * 	This function creates a new image with the appropriate type, compression and
 * 	color depth and sends it to gray() function to test if the function is working
 * 	properly and produces the desired results.
 */

#ifdef DEBUG

#include "test.h"

/** @brief Passes a non-existent file's name to the module to test the
 * 		   fopen's error handling.
 *
 *  @return EXPECTED when everything goes as expected, else UNEXPECTED.
 */
PRIVATE int openFile() {

	char *fn_of = "randomfilename.bmp";

	printf("<openFile()>\n");
	if(gray(fn_of)) {
		printf("</openFile()>\n\n");
		return EXPECTED;
	}

	return UNEXPECTED;
}

/** @brief Creates an empty binary file and checks if the fread validation
 *         checks are done correctly.
 *
 *  @return EXPECTED when everything goes as expected, else UNEXPECTED.
 */
PRIVATE int readFile() {

	FILE *test_fp = NULL;
	char *fn_rf = "test_readFile.bmp";

	if((test_fp = fopen(fn_rf, "wb")) == NULL) {
		printf("**Error, readFile() driver unable create test file**\n");
		return UNEXPECTED;
	}

	fclose(test_fp);

	printf("<readFile()>\n");
	if(gray(fn_rf)) {
		printf("</readFile()>\n\n");
		if(remove(fn_rf)) {
			printf("**Error, readFile() driver unable to remove %s**", fn_rf);
			return UNEXPECTED;
		}
		return EXPECTED;
	}

	if(remove(fn_rf)) {
		printf("**Error, readFile() driver unable to remove %s**", fn_rf);
		return UNEXPECTED;
	}

	return UNEXPECTED;
}

/** @brief Creates a random image and uses it to test the bitmap type
 *         validation check.
 *
 *  This driver creates a random image with a type of file different than
 *  BM to simulate a non bitmap file.
 *
 *  @return EXPECTED when everything goes as expected, else UNEXPECTED.
 */
PRIVATE int isBM() {

	char *fn_ib = "test_isBM.bmp";

	BITMAPFILEHEADER f_head;
	BITMAPINFOHEADER i_head;

	makeFileH('O', 'K', &f_head);
	makeInfoH(4, 2, 24, 0, &i_head);

	makeImage(fn_ib, f_head, i_head);

	printf("<isBM()>\n");
	if(gray(fn_ib)) {
		printf("</isBM()>\n\n");
		if(remove(fn_ib)) {
			printf("**Error, isBM() driver unable to remove %s**", fn_ib);
			return UNEXPECTED;
		}
		return EXPECTED;
	}

	if(remove(fn_ib)) {
		printf("**Error, isBM() driver unable to remove %s**", fn_ib);
		return UNEXPECTED;
	}

	return UNEXPECTED;
}

/** @brief Creates a random image and uses it to test the compression validation
 * 		   check.
 *
 *  This driver creates a random image with a biCompression value different
 *  than 0, simulating a compressed bitmap image.
 *
 *  @return EXPECTED when everything goes as expected, else UNEXPECTED.
 */
PRIVATE int hasCompress() {

	char *fn_hc = "test_hasCompress.bmp";

	BITMAPFILEHEADER f_head;
	BITMAPINFOHEADER i_head;

	makeFileH('B', 'M', &f_head);
	makeInfoH(4, 2, 24, 5, &i_head);

	makeImage(fn_hc, f_head, i_head);

	printf("<hasCompress()>\n");
	if(gray(fn_hc)) {
		printf("</hasCompress()>\n\n");
		if(remove(fn_hc)) {
			printf("**Error, hasCompress() driver unable to remove %s**",fn_hc);
			return UNEXPECTED;
		}
		return EXPECTED;
	}

	if(remove(fn_hc)) {
		printf("**Error, hasCompress() driver unable to remove %s**",fn_hc);
		return UNEXPECTED;
	}

	return UNEXPECTED;
}

/** @brief Creates a random image and uses it to test the 24-bit validation
 * 		   check.
 *
 *  This driver creates a random image with a bit depth value different
 *  than 24, simulating a not 24-bit bitmap image.
 *
 *  @return EXPECTED when everything goes as expected, else UNEXPECTED.
 */
PRIVATE int isPixel24bit() {

	char *fn_ip = "test_isPixel24bit.bmp";

	BITMAPFILEHEADER f_head;
	BITMAPINFOHEADER i_head;

	makeFileH('B', 'M', &f_head);
	makeInfoH(4, 2, 64, 0, &i_head);

	makeImage(fn_ip, f_head, i_head);

	printf("<isPixel24bit>\n");
	if(gray(fn_ip)) {
		printf("</isPixel24bit>\n\n");
		if(remove(fn_ip)) {
			printf("**Error, isPixel24bit() driver unable to remove %s**",fn_ip);
			return UNEXPECTED;
		}
		return EXPECTED;
	}

	if(remove(fn_ip)) {
		printf("**Error, isPixel24bit() driver unable to remove %s**", fn_ip);
		return UNEXPECTED;
	}

	return UNEXPECTED;
}

/** @brief This functions creates images of every possible byte padding.
 *
 *  @return A pointer to an int array that records for which paddings errors
 *          have occured, else i returns NULL.
 */
PRIVATE int *paddingCases() {

	int *errors, *p;
	short flag = 0;
	char *fn_pc0 = "test_padd_0.bmp";
	char *fn_pc1 = "test_padd_1.bmp";
	char *fn_pc2 = "test_padd_2.bmp";
	char *fn_pc3 = "test_padd_3.bmp";

	BITMAPFILEHEADER f_head;
	BITMAPINFOHEADER i_head;

	if((errors = (int *)malloc(sizeof(int)*4)) == NULL) {
		printf("**Error, not enough memory allocated.\n");
		EXIT_FAILURE;
	}

	printf("<paddingCases()>\n\n");

	makeFileH('B', 'M', &f_head);

	// padding = 0;
	makeInfoH(4, 2, 24, 0, &i_head);
	makeImage(fn_pc0, f_head, i_head);
	if(gray(fn_pc0)) {
		flag = 1;
		errors[0] = -1;
		printf("**Error, unexpected behavior for 0 bytes padded bitmap image**\n");
	}

	if(remove(fn_pc0)) {
		printf("**Error, paddingCases() driver unable to remove %s**",fn_pc0);
	}

	// padding = 1;
	makeInfoH(5, 2, 24, 0, &i_head);
	makeImage(fn_pc1, f_head, i_head);
	if(gray(fn_pc1)) {
		flag = 1;
		errors[1] = -1;
		printf("**Error, unexpected behavior for 1 byte padded bitmap image**\n");
	}

	if(remove(fn_pc1)) {
		printf("**Error, paddingCases() driver unable to remove %s**",fn_pc1);
	}

	// padding = 2;
	makeInfoH(6, 2, 24, 0, &i_head);
	makeImage(fn_pc2, f_head, i_head);
	if(gray(fn_pc2)) {
		flag = 1;
		errors[2] = -1;
		printf("**Error, unexpected behavior for 2 bytes padded bitmap image**\n");
	}

	if(remove(fn_pc2)) {
		printf("**Error, paddingCases() driver unable to remove %s**",fn_pc2);
	}

	// padding = 3;
	makeInfoH(3, 2, 24, 0, &i_head);
	makeImage(fn_pc3, f_head, i_head);
	if(gray(fn_pc3)) {
		flag = 1;
		errors[3] = -1;
		printf("**Error, unexpected behavior for 3 bytes padded bitmap image**\n");
	}

	if(remove(fn_pc3)) {
		printf("**Error, paddingCases() driver unable to remove %s**",fn_pc3);
	}

	printf("</paddingCases>\n\n");

	if(!flag) {
		free(errors);
		errors = NULL;
	}

	return errors;
}

/** @brief Creates an image and uses it to test the whole module.
 *
 *  This driver receives various parameters for the creation of a specific
 *  bitmap image which will be the test dummy for this module.
 *
 *  @return EXPECTED when everything goes as expected, else UNEXPECTED.
 */
PRIVATE int isWorking() {

	char *fn_iw = "test_isWorking.bmp";
	byte t1, t2;
	dword wid, hei, com;
	word bc;

	BITMAPFILEHEADER f_head;
	BITMAPINFOHEADER i_head;

	printf("<isWorking>\n\n");

	printf("Creating New Image\n");
	printf("Input bfType1: ");
	scanf("%c", &t1);
	printf("Input bfType2: ");
	scanf(" %c", &t2);
	printf("Input biBitCount: ");
	scanf("%hd", &bc);
	printf("Input biCompression: ");
	scanf("%d", &com);
	printf("Input biWidth: ");
	scanf("%d", &wid);
	printf("Input biHeight: ");
	scanf("%d",&hei);
	printf("\n");

	makeFileH(t1, t2, &f_head);
	makeInfoH(wid, hei, bc, com, &i_head);

	makeImage(fn_iw, f_head, i_head);

	if(!gray(fn_iw)) {
		printf("(Open the new %s for verification)\n\n",fn_iw);
		printf("</isWorking>\n\n");
		return EXPECTED;
	}

	printf("</isWorking>\n\n");
	return UNEXPECTED;
}

int main() {

	FILE *rep_file = NULL;
	int *pad_errs = NULL;
	dword i;
	char c, choice = '\0';

	if((rep_file = fopen("gray_report.txt", "w+")) == NULL) {
		printf("**Error, unable to create report_list.txt**\n");
		return EXIT_FAILURE;
	}

	/* Printing on the screen and in the report file */
	fprintf(rep_file, "====================================\n");
	fprintf(rep_file, "** Report for gray.c test drivers **\n");
	fprintf(rep_file, "====================================\n\n");
	fprintf(rep_file, "#  Driver's Name \t\t\tStatus\n");
	fprintf(rep_file, "-----------------------------------------------\n");

	printf("\n\n*******************************\n");
	printf("   <BEGIN of gray.c Testing>\n");
	printf("*******************************\n\n");

	if(openFile()) {
		fprintf(rep_file, "1. openFile() \t\t\t\tSUCCESS\n");
	}
	else {
		fprintf(rep_file, "1. openFile() \t\t\t\tFAILURE\n");
	}

	if(readFile()) {
		fprintf(rep_file, "2. readFile() \t\t\t\tSUCCESS\n");
	}
	else {
		fprintf(rep_file, "2. readFile() \t\t\t\tFAILURE\n");
	}

	if(isBM()) {
		fprintf(rep_file, "3. isBM() \t\t\t\tSUCCESS\n");
	}
	else {
		fprintf(rep_file, "3. isBM() \t\t\t\tFAILURE\n");
	}

	if(hasCompress()) {
		fprintf(rep_file, "4. hasCompress() \t\t\tSUCCESS\n");
	}
	else {
		fprintf(rep_file, "4. hasCompress() \t\t\tFAILURE\n");
	}

	if(isPixel24bit()) {
		fprintf(rep_file, "5. isPixel24bit() \t\t\tSUCCESS\n");
	}
	else {
		fprintf(rep_file, "5. isPixel24bit() \t\t\tFAILURE\n");
	}

	if((pad_errs = paddingCases()) == NULL) {
		fprintf(rep_file, "6. paddingCases() - Padding Bytes: 0\tSUCCESS\n");
		fprintf(rep_file, "6. paddingCases() - Padding Bytes: 1\tSUCCESS\n");
		fprintf(rep_file, "6. paddingCases() - Padding Bytes: 2\tSUCCESS\n");
		fprintf(rep_file, "6. paddingCases() - Padding Bytes: 3\tSUCCESS\n");
	}
	else {
		for(i=0; i<4; i++) {
			if(pad_errs[i] == -1) {
				fprintf(rep_file, "6. paddingCases() - Padding Bytes: %d\tFAILURE\n", i);
			}
		}
		free(pad_errs);

	}

	if(isWorking()) {
		fprintf(rep_file, "7. isWorking() \t\t\t\tSUCCESS\n");
	}
	else {
		fprintf(rep_file, "7. isWorking() \t\t\t\tFAILURE\n");
	}

	rewind(rep_file);

	printf("\n");

	while((c = getc(rep_file)) != EOF) {
		printf("%c", c);
	}

	fclose(rep_file);

	// Dialogue message for a choice to keep the report file or delete it.
	printf("\n>> Do you want to keep the report file? (y/n): ");
	scanf(" %c", &choice);

	// Validation check for choice variable - only 'y' or 'n' are accepted.
	while(choice != 'y' && choice != 'n') {
		printf("**Error, %c is an invalid choice, 'y' for YES -- 'n' for NO**\n",choice);
		printf(">> Do you want to keep the report file? (y/n): ");
		scanf(" %c", &choice);
	}

	if(choice == 'n') {
		if(remove("gray_report.txt")) {
			printf("**Error, unable to remove the report file**");
			return EXIT_FAILURE;
		}
	}

	printf("\n\n******************************\n");
	printf("   </END of gray.c Testing>\n");
	printf("******************************\n\n");

	return EXIT_SUCCESS;

}

#endif
