; Auto-generated. Do not edit!


(cl:in-package ex2_pkg_msgs-msg)


;//! \htmlinclude joints.msg.html

(cl:defclass <joints> (roslisp-msg-protocol:ros-message)
  ((positions
    :reader positions
    :initarg :positions
    :type (cl:vector ex2_pkg_msgs-msg:position)
   :initform (cl:make-array 0 :element-type 'ex2_pkg_msgs-msg:position :initial-element (cl:make-instance 'ex2_pkg_msgs-msg:position))))
)

(cl:defclass joints (<joints>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <joints>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'joints)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name ex2_pkg_msgs-msg:<joints> is deprecated: use ex2_pkg_msgs-msg:joints instead.")))

(cl:ensure-generic-function 'positions-val :lambda-list '(m))
(cl:defmethod positions-val ((m <joints>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ex2_pkg_msgs-msg:positions-val is deprecated.  Use ex2_pkg_msgs-msg:positions instead.")
  (positions m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <joints>) ostream)
  "Serializes a message object of type '<joints>"
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'positions))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'positions))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <joints>) istream)
  "Deserializes a message object of type '<joints>"
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'positions) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'positions)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'ex2_pkg_msgs-msg:position))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<joints>)))
  "Returns string type for a message object of type '<joints>"
  "ex2_pkg_msgs/joints")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'joints)))
  "Returns string type for a message object of type 'joints"
  "ex2_pkg_msgs/joints")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<joints>)))
  "Returns md5sum for a message object of type '<joints>"
  "03d6253bad558f8093b6baf0e2461947")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'joints)))
  "Returns md5sum for a message object of type 'joints"
  "03d6253bad558f8093b6baf0e2461947")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<joints>)))
  "Returns full string definition for message of type '<joints>"
  (cl:format cl:nil "position[] positions~%================================================================================~%MSG: ex2_pkg_msgs/position~%uint8 position~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'joints)))
  "Returns full string definition for message of type 'joints"
  (cl:format cl:nil "position[] positions~%================================================================================~%MSG: ex2_pkg_msgs/position~%uint8 position~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <joints>))
  (cl:+ 0
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'positions) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <joints>))
  "Converts a ROS message object to a list"
  (cl:list 'joints
    (cl:cons ':positions (positions msg))
))
