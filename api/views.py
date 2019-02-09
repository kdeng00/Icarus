from django.shortcuts import render
from rest_framework import status
from rest_framework.response import Response
from rest_framework.views import APIView
from .models import Song
from .serializers import SongSerialzer

class UploadList(APIView):

    def post(self, request, format=None):
        serializer = SongSerialzer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response({
                'demo':'success'
                })

        return Response({
        'demo': 'failure'
        }, status=status.HTTP_400_BAD_REQUEST)

    def get(self, request, format=None):
        songs = Song.objects.all()
        serializer = SongSerialzer(songs, many=True)
        return Response(serializer.data)

class UploadDetail(APIView):

    def get_object(self, pk):
        try:
            return Song.objects.get(pk=pk)
        except Song.DoesNotExist:
            raise Http404

