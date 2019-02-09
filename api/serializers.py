from rest_framework import serializers
from .models import Song

class SongSerialzer(serializers.ModelSerializer):
    class Meta:
        model = Song
        fields = ('id', 'title', 'albumn', 'year', 'genre', 'track_number', 'file')
