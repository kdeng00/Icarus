from django.urls import path
from rest_framework.urlpatterns import format_suffix_patterns
from api import views

urlpatterns = [
    path('song/', views.UploadList().as_view()),
    path('upload/song/', views.UploadList().as_view())        
]

urlpatterns = format_suffix_patterns(urlpatterns)
