#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>

// 2차원 배열 동적할당 위함
unsigned char** imageMatrix;
// 이진영상에서
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
	int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {

	int i, j, sum = 0;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if (i != 0 || j != 0)
				sum += (imageMatrix[row + i][col + j] == imagePixel);
		}
	}

	return sum;
}

int getBWTransitions(int row, int col) {
	return 	((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
		+ (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
		+ (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
		+ (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
		+ (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
		+ (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
		+ (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
		+ (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

// Thining
int zhangSuenTest1(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel)
		&& (imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col - 1] == blankPixel));
}

int zhangSuenTest2(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row][col - 1] == blankPixel)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel));
}

void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) {

	int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;

	pixel* markers;

	imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));

	for (i = 0; i < rows; i++) {
		imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
		for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
	}

	endRow = rows - 2;
	endCol = cols - 2;
	do {
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		if (processed == 0)
			processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
	} while (processed == 1);


	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			output[i * cols + j] = imageMatrix[i][j];
		}
	}
}

void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}

// 밝기 조절
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] = Img[i] + Val;
	}
}

// Contrast 조절
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto)
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

// 스트레칭
void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}
}

// 평탄화
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}

// 이진화
void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

// Gonzalez 자동 이진화 알고리즘
int GonzalezBinThresh(int* Histo) {
	int Min = 255, Max = 0;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Min = (i < Min) ? i : Min;
			Max = (i > Max) ? i : Max;
		}
	}
	int T = (Min + Max) / 2;
	int Init;
	// 영상의 최소값, 최대값 구하기
	do {
		Init = T;
		int Sum1 = 0, Sum2 = 0, Cnt1 = 0, Cnt2 = 0;
		double Avg1, Avg2;

		for (int i = 0; i <= T; i++) {
			Sum1 += i * Histo[i];
			Cnt1 += Histo[i];
		}
		Avg1 = (double)Sum1 / Cnt1;

		for (int i = T + 1; i < 256; i++) {
			Sum2 += i * Histo[i];
			Cnt2 += Histo[i];
		}
		Avg2 = (double)Sum2 / Cnt2;
		//새로운 경계값
		T = (Avg1 + Avg2) / 2;
	} while (abs(T - Init) > 3);

	return T;
}

// 박스 평활화
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { 0.11111,0.11111 ,0.11111,
							0.11111,0.11111 ,0.11111,
							0.11111,0.11111 ,0.11111 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// 가우시안 평활화
void GaussAvgConv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { 0.0625,0.125 ,0.0625,
							0.125,0.25 ,0.125,
							0.0625,0.125 ,0.0625 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 X
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { -1.0,0 ,1.0,
							-1.0,0 ,1.0,
							-1.0,0 ,1.0 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			// 0~765 ===>> 0~255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 Y
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { -1.0,-1.0,-1.0,
							0.0,0.0,0.0,
							1.0,1.0,1.0 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			// 0~765 ===>> 0~255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

// Sobel 마스크 X
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { -1.0,0 ,1.0,
							-2.0,0 ,2.0,
							-1.0,0 ,1.0 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			// 0~1020 ===>> 0~255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

// Sobel 마스크 Y
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { -1.0,-2.0,-1.0,
							0.0,0.0,0.0,
							1.0,2.0,1.0 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			// 0~1020 ===>> 0~255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

// 라플라시안
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { -1.0,-1.0,-1.0,
							-1.0,8.0,-1.0,
							-1.0,-1.0,-1.0 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}
			// 0~2040 ===>> 0~255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

// 라플라시안이 노이즈에 얼마나 민감한지 확인
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) {
	double Kennel[3][3] = { -1.0,-1.0,-1.0,
							-1.0,9.0,-1.0,
							-1.0,-1.0,-1.0 };
	double SumProduct = 0.0;
	// margin을 두기 위해 1부터 시작해서 W(H) -1 전에 종료
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) { // 여기까지 center 화소를 나타냄
			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) { // center 화소의 주변부를 계산하기 위한 for문 2개
					SumProduct += Img[(i + m) * W + (j + n)] * Kennel[m + 1][n + 1];
				}
			}

			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	if (hInfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W * H * 3, fp);
	}
	else {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W * H, fp);
	}
	fclose(fp);
}

void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

// 중앙값 구하는 함수
BYTE Median(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}

// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;

	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;


	for (k = 0; k < height * width; k++) coloring[k] = 0;  

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; 
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;

	// 가장 면적이 넓은 영역을 찾아내기 위함
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장 (size filtering)
		//if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // 특정 면적이상되는 영역만 출력
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}

// 이진화 이미지를 경계검출하는 함수
void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0) // 전경화소라면
			{
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 &&
					Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0)) // 4방향 화소 중 하나라도 전경화소가 아니라면
					Out[i * W + j] = 255;
			}
		}
	}
}

// Img: 사각형을 그릴 이미지배열, W: 영상 가로사이즈, H: 영상 세로사이즈,
// LU_X: 사각형의 좌측상단 X좌표, LU_Y: 사각형의 좌측상단 Y좌표,
// RD_X: 사각형의 우측하단 X좌표, LU_   Y: 사각형의 우측하단 Y좌표.
void DrawRectOutline(BYTE* Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	for (int i = LU_X; i < RD_X; i++) Img[LU_Y * W * 3 + i * 3 + 2] = 255;
	for (int i = LU_X; i < RD_X; i++) Img[RD_Y * W * 3 + i * 3 + 2] = 255;
	for (int i = LU_Y; i < RD_Y; i++) Img[i * W * 3 + LU_X * 3 + 2] = 255;
	for (int i = LU_Y; i < RD_Y; i++) Img[i * W * 3 + RD_X * 3 + 2] = 255;
	
}


// Img: 가로/세로 라인을 그릴 이미지배열, W: 영상 가로사이즈, H: 영상 세로사이즈,
// Cx: 가로/세로 라인이 교차되는 지점의 X좌표
// Cy: 가로/세로 라인이 교차되는 지점의 Y좌표
void DrawCrossLine(BYTE* Img, int W, int H, int Cx, int Cy) {
	for (int i = 0; i < W - 1; i++) { // horizontal line
		Img[Cy * W + i] = 255;
	}
	for (int i = 0; i < H - 1; i++) { // vertical line
		Img[i * W + Cx] = 255;
	}
}
// 무게중심 구하는 함수
void Obtain2DCenter(BYTE* Image, int W, int H, int* Cx, int* Cy) {
	int SumX = 0, SumY = 0;
	int cnt = 0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Image[i * W + j] == 0) // 동공영역이면
			{
				SumX += j;
				SumY += i;
				cnt++;
			}
		}
	}
	if (cnt == 0) cnt = 1;
	*Cx = SumX / cnt;
	*Cy = SumY / cnt;
	//printf("%d %d\n", Cx, Cy);
}

// 외접 직사각형 구하는 함수
void Obtain2DBoundingBox(BYTE* Image, int W, int H, int* LUX, int* LUY, int* RDX, int* RDY) {
	int flag = 0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((Image[i * W * 3 + j * 3] != 0) || (Image[i * W * 3 + j * 3 + 1] != 0) || (Image[i * W * 3 + j * 3 + 2] != 0)) {
				*LUY = i;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}

	flag = 0;
	for (int i = H - 1; i >= 0; i--) {
		for (int j = W - 1; j >= 0; j--) {
			if ((Image[i * W * 3 + j * 3] != 0) || (Image[i * W * 3 + j * 3 + 1] != 0) || (Image[i * W * 3 + j * 3 + 2] != 0)) {
				*RDY = i;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}

	flag = 0;
	for (int j = 0; j < W; j++) {
		for (int i = 0; i < H; i++) {
			if ((Image[i * W * 3 + j * 3] != 0) || (Image[i * W * 3 + j * 3 + 1] != 0) || (Image[i * W * 3 + j * 3 + 2] != 0)) {
				*LUX = j;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}

	flag = 0;
	for (int j = W - 1; j >= 0; j--) {
		for (int i = H - 1; i >= 0; i--) {
			if ((Image[i * W * 3 + j * 3] != 0) || (Image[i * W * 3 + j * 3 + 1] != 0) || (Image[i * W * 3 + j * 3 + 2] != 0)) {
				*RDX = j;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}
}

// VerticalFilp
void VerticalFilp(BYTE* Img, int W, int H) {
	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W; j++) {
			swap(&Img[i * W + j], &Img[(H - 1 - i) * W + j]);
		}
	}
}

// HorizontalFlip
void HorizontalFlip(BYTE* Img, int W, int H) {
	for (int i = 0; i < W / 2; i++) {
		for (int j = 0; j < H; j++) {
			swap(&Img[j * W + i], &Img[j * W + (W - 1 - i)]);
		}
	}
}

// Translation (이동)
void Translation(BYTE* Image, BYTE* Output, int W, int H, int Tx, int Ty) {
	Ty *= -1;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0))
				Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j];
		}
	}
}

// Scaling
void Scaling(BYTE* Image, BYTE* Output, int W, int H, double SF_X, double SF_Y) {
	int tmpX, tmpY;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(j / SF_X);
			tmpY = (int)(i / SF_Y);
			if (tmpY < H && tmpX < W)
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}

// Rotation
void Rotation(BYTE* Image, BYTE* Output, int W, int H, int Angle) {
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * j + sin(Radian) * i);
			tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}

// 1픽셀 점찍는 함수
void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
	Image[Y * W * 3 + X * 3] = B; //Blue 성분
	Image[Y * W * 3 + X * 3 + 1] = G; //Green 성분
	Image[Y * W * 3 + X * 3 + 2] = R; //Red 성분
}

// RGB를 YCbCr로 변환
void RGB2YCbCr(BYTE* Image, BYTE* Y, BYTE* Cb, BYTE* Cr, int W, int H) {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            int index = i * W * 3 + j * 3;
            Y[i * W + j] = (BYTE)(0.299 * Image[index + 2] + 0.587 * Image[index + 1] + 0.114 * Image[index]);
            Cb[i * W + j] = (BYTE)(-0.16874 * Image[index + 2] - 0.3313 * Image[index + 1] + 0.5 * Image[index] + 128);
            Cr[i * W + j] = (BYTE)(0.5 * Image[index + 2] - 0.4187 * Image[index + 1] - 0.0813 * Image[index] + 128);
        }
    }
}

// 침식
void Erosion(BYTE* Image, BYTE* Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 255) // 전경화소라면
			{
				if (!(Image[(i - 1) * W + j] == 255 &&
					Image[(i + 1) * W + j] == 255 &&
					Image[i * W + (j - 1)] == 255 &&
					Image[i * W + (j + 1)] == 255)) { // 4주변화소가 모두 전경화소가 아니라면
					Output[i * W + j] = 0;
				}
				else Output[i * W + j] = 255;
			}
			else Output[i * W + j] = 0;
		}
	}
}

// 팽창
void Dilation(BYTE* Image, BYTE* Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 0) // 배경화소라면
			{
				if (!(Image[(i - 1) * W + j] == 0 &&
					Image[(i + 1) * W + j] == 0 &&
					Image[i * W + (j + 1)] == 0 &&
					Image[i * W + (j - 1)] == 0)) { // 4주변화소가 모두 배경화소가 아니라면
					Output[i * W + j] = 255;
				}
				else Output[i * W + j] = 0;
			}
			else Output[i * W + j] = 255;
		}
	}
}

// Thinging된 이미지에서 분기점, 끝점 표시 함수
void FeatureExtractThinImage(BYTE* Image, BYTE* Output, int W, int H) {
	int count = 0;
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 0) {
				for (int m = -1; m <= 1; m++) {
					for (int n = -1; n <= 1; n++) {
						if (Image[(i + m) * W + (j + n)] == 0)
							count++;
					}
				}

				if ((count == 2) || (count >= 4))
				{
					Output[i * W + j] = 255;
					Output[(i - 1) * W + j] = 128;
					Output[(i + 1) * W + j] = 128;
					Output[i * W + j - 1] = 128;
					Output[i * W + j + 1] = 128;
				}
				count = 0;
			}
		}
	}
}


int main()
{
	BITMAPFILEHEADER hf; 
	BITMAPINFOHEADER hInfo; 
	RGBQUAD hRGB[256];
	FILE* fp;
	fp = fopen("dilation.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	int H = hInfo.biHeight, W = hInfo.biWidth;

	BYTE* Image;
	BYTE* Output;
	BYTE* Temp = (BYTE*)malloc(ImgSize);

	if (hInfo.biBitCount == 24) { //트루컬러 
		Image = (BYTE*)malloc(ImgSize * 3);
		Output = (BYTE*)malloc(ImgSize * 3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);

	}
	else { //인덱스(그레이)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE*)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
	fclose(fp);

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };

	// FillColor(Image, 100, 200, W, H, 0, 255, 255);
	// 그라데이션 가로 띠 만들기
	/*
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}
	double wt;
	for (int i = 0; i < 160; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)(255 * (1.0 - wt)); //Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; //Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; //Red
		}
	}
	for (int i = 160; i < 320; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3 + 1] = (BYTE)(255 * (1.0 - wt)); //Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; //Red
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; //Blue
		}
	}
	for (int i = 320; i < 480; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3 + 2] = (BYTE)(255 * (1.0 - wt)); //Red
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; //Green
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; //Blue
		}
	}
	*/
	// 그라데이션 만들기
	/*
	double wt;
	for (int a = 0; a < 120; a++) {
		for (int i = 0; i < W; i++) {
			wt = i / (double)(W - 1);
			Image[a * W * 3 + i * 3] = (BYTE)(255 * (1.0 - wt));
			Image[a * W * 3 + i * 3 + 1] = 0;
			Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * (wt));
		}
	}
	*/
	 // Red값이 큰 화소만 masking (R, G, B 모델 기준)
	/*for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Image[i * W * 3 + j * 3 + 2] > 130 &&
				Image[i * W * 3 + j * 3 + 1] < 50 &&
				Image[i * W * 3 + j * 3 + 1] < 100 ) {
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
			}
			else
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
		}
	}*/
	/*
	BYTE* Y = (BYTE*)malloc(ImgSize);
	BYTE* Cb = (BYTE*)malloc(ImgSize);
	BYTE* Cr = (BYTE*)malloc(ImgSize);
	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	*/
	/*
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Cb[i * W + j] > 85 && 
				Cb[i * W + j] < 135 && 
				Cr[i * W + j] > 135 &&
				Cr[i * W + j] < 185) {
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
			}
			else {
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
			}
		}
	}
	*/
	/*
	// 외접 직사각형 그리기
	int LUX, LUY, RDX, RDY;
	Obtain2DBoundingBox(Output, W, H, &LUX, &LUY, &RDX, &RDY);
	printf("%d %d %d %d\n", LUX, LUY, RDX, RDY);
	DrawRectOutline(Output, W, H, LUX, LUY, RDX, RDY);
	*/
	/*
	free(Y);
	free(Cb);
	free(Cr);
	*/

	Dilation(Image, Output, W, H);
	Dilation(Output, Image, W, H);
	Dilation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Temp, H, W);

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Output[i * W + j] = Temp[i * W + j];
		}

	}
	FeatureExtractThinImage(Temp, Output, H, W);


	SaveBMPFile(hf, hInfo, hRGB, Output, W, H, "output.bmp");

	free(Image);
	free(Output);
	free(Temp);
	return 0;
}