// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef FACE_ALIGN_H
#define FACE_ALIGN_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


namespace FacePreprocess {

    cv::Mat meanAxis0(const cv::Mat &src)
    {
        int num = src.rows;
        int dim = src.cols;

        // x1 y1
        // x2 y2

        cv::Mat output(1,dim,CV_32F);
        for(int i = 0 ; i <  dim; i ++)
        {
            float sum = 0 ;
            for(int j = 0 ; j < num ; j++)
            {
                sum+=src.at<float>(j,i);
            }
            output.at<float>(0,i) = sum/num;
        }

        return output;
    }

    cv::Mat elementwiseMinus(const cv::Mat &A,const cv::Mat &B)
    {
        cv::Mat output(A.rows,A.cols,A.type());

        assert(B.cols == A.cols);
        if(B.cols == A.cols)
        {
            for(int i = 0 ; i <  A.rows; i ++)
            {
                for(int j = 0 ; j < B.cols; j++)
                {
                    output.at<float>(i,j) = A.at<float>(i,j) - B.at<float>(0,j);
                }
            }
        }
        return output;
    }


    cv::Mat varAxis0(const cv::Mat &src)
    {
        cv:Mat temp_ = elementwiseMinus(src,meanAxis0(src));
        cv::multiply(temp_ ,temp_ ,temp_ );
        return meanAxis0(temp_);

    }



    int MatrixRank(cv::Mat M)
    {
        Mat w, u, vt;
        SVD::compute(M, w, u, vt);
        Mat1b nonZeroSingularValues = w > 0.0001;
        int rank = countNonZero(nonZeroSingularValues);
        return rank;

    }

//    References
//    ----------
//    .. [1] "Least-squares estimation of transformation parameters between two
//    point patterns", Shinji Umeyama, PAMI 1991, DOI: 10.1109/34.88573
//
//    """
//
//    Anthor:Jack Yu
    cv::Mat similarTransform(cv::Mat src,cv::Mat dst) {
        int num = src.rows;
        int dim = src.cols;
        cv::Mat src_mean = meanAxis0(src);
        cv::Mat dst_mean = meanAxis0(dst);
        cv::Mat src_demean = elementwiseMinus(src, src_mean);
        cv::Mat dst_demean = elementwiseMinus(dst, dst_mean);
        cv::Mat A = (dst_demean.t() * src_demean) / static_cast<float>(num);
        cv::Mat d(dim, 1, CV_32F);
        d.setTo(1.0f);
        if (cv::determinant(A) < 0) {
            d.at<float>(dim - 1, 0) = -1;

        }
        cv::Mat T = cv::Mat::eye(dim + 1, dim + 1, CV_32F);
        cv::Mat U, S, V;
        cv::SVD::compute(A, S,U, V);

        // the SVD function in opencv differ from scipy .


        int rank = MatrixRank(A);
        if (rank == 0) {
            assert(rank == 0);

        } else if (rank == dim - 1) {
            if (cv::determinant(U) * cv::determinant(V) > 0) {
                T.rowRange(0, dim).colRange(0, dim) = U * V;
            } else {
                int s = d.at<float>(dim - 1, 0) = -1;
                d.at<float>(dim - 1, 0) = -1;

                T.rowRange(0, dim).colRange(0, dim) = U * V;
                cv::Mat diag_ = cv::Mat::diag(d);
                cv::Mat twp = diag_*V; //np.dot(np.diag(d), V.T)
                cv::Mat B = cv::Mat::zeros(3, 3, CV_8UC1);
                cv::Mat C = B.diag(0);
                T.rowRange(0, dim).colRange(0, dim) = U* twp;
                d.at<float>(dim - 1, 0) = s;
            }
        }
        else{
            cv::Mat diag_ = cv::Mat::diag(d);
            cv::Mat twp = diag_*V.t(); //np.dot(np.diag(d), V.T)
            cv::Mat res = U* twp; // U
            T.rowRange(0, dim).colRange(0, dim) = U *  diag_ * V;
        }
        cv::Mat var_ = varAxis0(src_demean);
        float val = cv::sum(var_).val[0];
        cv::Mat res;
        cv::multiply(d,S,res);
        float scale =  1.0/val*cv::sum(res).val[0];
        cv::Mat  temp1 = T.rowRange(0, dim).colRange(0, dim) * src_mean.t();
        cv::Mat  temp2 = scale * temp1;
        cv::Mat  temp3 = dst_mean - temp2.t();
        T.at<float>(0,2) = temp3.at<float>(0);
        T.at<float>(1,2) = temp3.at<float>(1);
        T.rowRange(0, dim).colRange(0, dim) *= scale; // T[:dim, :dim] *= scale

        return T;
    }


    int face_align(const cv::Mat& src, const std::vector<FaceObject>& faceobjects, std::vector<cv::Mat>& aligned_faces)
    {
        cv::Size face_size(112, 112);
        float points_dst[5][2] = {
            {30.2946f + 8.0f, 51.6963f},
            {65.5318f + 8.0f, 51.5014f},
            {48.0252f + 8.0f, 71.7366f},
            {33.5493f + 8.0f, 92.3655f},
            {62.7299f + 8.0f, 92.2041f}
        };
                    

        int face_num = faceobjects.size();
        if (face_num>0)
        {
            aligned_faces.resize(face_num);
            for(size_t i=0; i<face_num; i++)
            {
                const FaceObject& obj = faceobjects[i];
                float points_src[5][2] = {
                    {obj.landmark[0].x, obj.landmark[0].y},
                    {obj.landmark[1].x, obj.landmark[1].y},
                    {obj.landmark[2].x, obj.landmark[2].y},
                    {obj.landmark[3].x, obj.landmark[3].y},
                    {obj.landmark[4].x, obj.landmark[4].y}
                };

                cv::Mat src_mat(5, 2, CV_32FC1, points_src);
                cv::Mat dst_mat(5, 2, CV_32FC1, points_dst);
                
                cv::Mat transform = similarTransform(src_mat, dst_mat);
 
                cv::Mat warped;
                //warped.create(face_size, CV_32FC3);
                cv::Mat transfer_mat = transform(cv::Rect(0, 0, 3, 2));
                cv::warpAffine(src, warped, transfer_mat, face_size, 1, 0, 0);

                //cv::imwrite("warped_"+std::to_string(i)+".jpg", warped);
                aligned_faces[i] = warped;
            }

        }

        return 0;

    }

}

#endif // FACE_ALIGN_H
