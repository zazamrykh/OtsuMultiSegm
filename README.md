# OtsuMultiSegm
Segmentation Otsu method generalized for arbitrary number of classes.
Method uses histogram to calculate optimal thresholds using maximisation of interclass variance.
Algorythm:
  1) Smooth image (for decreasing noise)
  2) Calculate hist
  3) Smooth hist
  4) Sort through all the postitions of thresholds
Difficult:
  Difficult and time do not cease with image size cease. It is because main algorythm depends on HIST_SIZE and num of classes. Image size depends on time of building histogram.
Difficult of main algorythm depends of number of classes like O(HIST_SIZE^number_of_classes) so you should not use this method for many classes.
How to use:
You should change defines in otsu_segm.h.
What is need:
OpenCV needs only for image loading, so you can simply rid of it and use some other library for loading and showing images.
