make; rm *.o; echo "Running cppb"; time ./cppb -n prod; echo "Done"; make clean; time make -j 8;
