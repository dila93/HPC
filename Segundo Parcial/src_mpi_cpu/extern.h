extern "C" {
    void mult_mat_CUDA(double *h_a, double *h_b, double *h_c, int height,int width_a, int width_b);
	void init_buf(double *init_buf, int y, int x);
	void MPI_Multiply(double *a, double *b, double *c, int rows, int y, int x);
	bool compare_resultado(double *mat_MPI, double *mat_CUDA, int y, int x);
}