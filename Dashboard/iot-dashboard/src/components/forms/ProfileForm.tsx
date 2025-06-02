'use client'
import { getDeviceListInfo, getTasks } from '@/actions'
import { Avatar, AvatarFallback, AvatarImage } from '../ui/avatar'
import { useGetUserSB } from '@/hooks/useGetUserSB'
import { useEffect } from 'react'

export const ProfileForm = () => {
  const res = useGetUserSB()
  const email = res.userInfo?.session.user.email
  const avatarUrl = res.userInfo?.session.user.user_metadata.avatar_url
  const userFullName = res.userInfo?.session.user.user_metadata.full_name

  useEffect(() => {
    // ! Test
    const test = async () => {
      try {
        const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))
        // const res = await getDeviceInfo('test', token)
        // const res = await getDeviceListInfo(1, 1, token)
        // const res = await getTasks('-846983168')
        // const data = await getTelemetryTb('2ae615b0-192d-11f0-862b-b726a398b624', token, ['pH'])
        console.log('res >>>', res)
        // console.log('data >>>', data)
      } catch (error) {
        console.log(error)
      }
    }

    test()
  })

  return (
    <div>
      <div className="flex items-center gap-5 justify-center m-10">
        <Avatar className="w-20 h-20">
          <AvatarImage src={avatarUrl} />
          <AvatarFallback className="bg-blue-300">AVATAR</AvatarFallback>
        </Avatar>
        <div className="flex flex-col gap-3">
          <h3 className="font-bold">Email:</h3>
          <p className="text-sm">{email}</p>
          <h3 className="font-bold">Full Name:</h3>
          <p className="text-sm">{userFullName ? userFullName : 'NA'}</p>
        </div>
      </div>
    </div>
  )
}
