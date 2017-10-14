<html>
<body>
<h1> Unix Directory Synchronization  </h1>

<p> C project which gets
two directory paths as parameters and recursively check for timestamps
and sizes of files under these directories and try to find the set of operations
to make these two directories identical. <br>


<p> There are be two modes of operation: <br>

<ul style="list-style-type:disc">

<li>In the union mode, the purpose is to synchronize two directories such
that missing files of a directory is updated with the other and common
files will be updated with the most recent (written later) one.
Resulting file list will be the union of the both directories. </li>
<li>In the intersect mode, the purpose is to synchronize two directories
in a minimal common set. Only files existing in both directories will
be kept and others will be erased. When timestamps are different for
two files the older one will overwrite the recent one. (You can think
this process as the backtracking to a previous synchronize situation)</li>

</ul>


<p> Your command syntax is as follows:
synchronize [-i] dir1 dir2 <br>

<p> Default mode of operation is union and ‘-i’ switches to the intersect mode
of operation.
Your output will be list of cp, rm and ln command lines to make the synchronization. <br>


</html>
</body>