double mapD2wD(double u, double umin, double umax)
{
  double uVal = (u-umin)/(umax-umin);
  return uVal;
}

double maprD2D(double val, double umin, double umax)
{
  double dVal = val*(umax-umin) + umin;
  return dVal;
}
