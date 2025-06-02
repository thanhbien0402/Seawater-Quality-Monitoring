'use client'
import Link from 'next/link'
import { Button, buttonVariants } from '@/components/ui/button'
import { signOut } from '@/actions'
import { Icons } from './icons'
import { ProfileModal } from './modals/ProfileModal'
import { SettingModal } from './modals/SettingModal'
import { ProfileDrawer } from './drawers/ProfileDrawer'
import { SettingDrawer } from './drawers/SettingDrawer'
import { TaskListModal } from './modals/TaskListModal'
import { Map } from 'lucide-react'
import { TaskListDrawer } from './drawers/TaskListDrawer'
import { OTAModal } from './modals/OTAModal'
import { OTADrawer } from './drawers/OTADrawer'
import { PersonalModal } from './modals/PersonalModal'
import { NotificationDeviceModal } from './modals/NotificationDeviceModal'
import { PersonalDrawer } from './drawers/PersonalDrawer'
import { NotificationDeviceDrawer } from './drawers/NotificationDeviceDrawer'

export function Navbar({ deviceId }: { deviceId: string }) {
  return (
    <div className="flex flex-col mt-3 mx-8">
      <div className="flex justify-between">
        <div className="flex items-center">
          <Link href={'/'}>
            <Icons.sunset className="w-12 h-12" />
          </Link>
        </div>
        <div className="flex items-center">
          <ul className="flex md:gap-1 items-center">
            <li className="hidden md:block">
              <Link href={'/map'} className={buttonVariants({ variant: "ghost" })}>
                <Map />
                Map
              </Link>
            </li>
            <li className="md:hidden">
              <Link href={'/map'} className={buttonVariants({ variant: "ghost" })}>
                <Map />
              </Link>
            </li>
            <li className="md:hidden">
              <TaskListDrawer deviceId={deviceId} />
            </li>
            <li className="hidden md:block">
              <TaskListModal deviceId={deviceId} />
            </li>
            <li className="hidden md:block">
              <NotificationDeviceModal deviceId={deviceId} />
            </li>
            <li className="md:hidden">
              <NotificationDeviceDrawer deviceId={deviceId} />
            </li>
            <li className="hidden md:block">
              <PersonalModal deviceId={deviceId} />
            </li>
            <li className="md:hidden">
              <PersonalDrawer deviceId={deviceId} />
            </li>
            <li className="hidden md:block">
              <OTAModal deviceId={deviceId} />
            </li>
            <li className="md:hidden">
              <OTADrawer deviceId={deviceId} />
            </li>
            <li>
              <form action={signOut}>
                <Button onClick={() => global?.window?.localStorage.removeItem('token')} className="gap-1">
                  <Icons.logout className="h-5 w-5" />
                  <p className="hidden md:block">Sign Out</p>
                </Button>
              </form>
            </li>
          </ul>
        </div>
      </div>
      <div className="w-full mt-2 h-[2px] bg-gradient-to-r from-transparent via-[#c3cbd8] to-transparent"></div>
    </div>
  )
}
