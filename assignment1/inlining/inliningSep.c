void mul_cpx_sep(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im){
  (*a_re) = (*b_re) * (*c_re);
  (*a_im) = (*b_im) * (*c_im);
}
